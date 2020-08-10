//===- DlDecoder.cpp --------------------------------------------*- C++ -*-===//
//
//  Copyright (C) 2019 GrammaTech, Inc.
//
//  This code is licensed under the GNU Affero General Public License
//  as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version. See the
//  LICENSE.txt file in the project root for license terms or visit
//  https://www.gnu.org/licenses/agpl.txt.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU Affero General Public License for more details.
//
//  This project is sponsored by the Office of Naval Research, One Liberty
//  Center, 875 N. Randolph Street, Arlington, VA 22203 under contract #
//  N68335-17-C-0700.  The content of the information does not necessarily
//  reflect the position or policy of the Government and no official
//  endorsement should be inferred.
//
//===----------------------------------------------------------------------===//

#include "DlDecoder.h"

#include <souffle/CompiledSouffle.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include "../AuxDataSchema.h"
#include "ExceptionDecoder.h"

namespace souffle
{
    template <class T>
    souffle::tuple &operator<<(souffle::tuple &t, const DlData<T> &data)
    {
        t << data.ea << data.content;
        return t;
    }

    souffle::tuple &operator<<(souffle::tuple &t, const ElfRelocation &ElfRelocation)
    {
        auto &[Addr, Type, Name, Addend] = ElfRelocation;
        t << Addr << Type << Name << Addend;
        return t;
    }

    souffle::tuple &operator<<(souffle::tuple &t, const DataDirectory &DataDirectory)
    {
        auto &[Type, Address, Size] = DataDirectory;
        t << Address << Size << Type;
        return t;
    }

    souffle::tuple &operator<<(souffle::tuple &t, const ImportEntry &ImportEntry)
    {
        auto &[Address, Ordinal, Function, Library] = ImportEntry;
        t << Address << Ordinal << Function << Library;
        return t;
    }

} // namespace souffle

std::string getFileFormatString(const gtirb::FileFormat format)
{
    switch(format)
    {
        case gtirb::FileFormat::COFF:
            return "COFF";
        case gtirb::FileFormat::ELF:
            return "ELF";
        case gtirb::FileFormat::PE:
            return "PE";
        case gtirb::FileFormat::IdaProDb32:
            return "IdaProDb32";
        case gtirb::FileFormat::IdaProDb64:
            return "IdaProDb64";
        case gtirb::FileFormat::XCOFF:
            return "XCOFF";
        case gtirb::FileFormat::MACHO:
            return "MACHO";
        case gtirb::FileFormat::RAW:
            return "RAW";
        case gtirb::FileFormat::Undefined:
        default:
            return "Undefined";
    }
}

const char *getISAString(gtirb::ISA format)
{
    switch(format)
    {
        case gtirb::ISA::X64:
            return "X64";
        case gtirb::ISA::ARM:
        case gtirb::ISA::ARM64:
            return "ARM";
        default:
            return "Undefined";
    }
}

unsigned int DlDecoder::getISAPointerSize() const
{
    // FIXME: Generalize this for all architectures, supported and unsupported.
    switch(CsHandle.getIsa())
    {
        case gtirb::ISA::X64:
            return 8;
        case gtirb::ISA::ARM64:
            return 8;
        case gtirb::ISA::ARM:
            return 4;
        default:
            throw std::runtime_error("Unsupported ISA ");
    }
}

void addSymbols(souffle::SouffleProgram *prog, const gtirb::Module &module)
{
    auto *rel = prog->getRelation("symbol");
    auto *SymbolInfo = module.getAuxData<gtirb::schema::ElfSymbolInfoAD>();
    for(auto &symbol : module.symbols())
    {
        souffle::tuple t(rel);
        if(auto address = symbol.getAddress())
            t << *address;
        else
            t << 0;
        ElfSymbolInfo Info = {0, "NOTYPE", "GLOBAL", "DEFAULT", 0};
        if(SymbolInfo)
        {
            auto found = SymbolInfo->find(symbol.getUUID());
            if(found == SymbolInfo->end())
                throw std::logic_error("Symbol " + symbol.getName()
                                       + " missing from elfSymbolInfo AuxData table");
            Info = found->second;
        }
        t << std::get<0>(Info) << std::get<1>(Info) << std::get<2>(Info) << std::get<4>(Info)
          << symbol.getName();
        rel->insert(t);
    }
}

void addSections(souffle::SouffleProgram *prog, const gtirb::Module &module)
{
    auto *rel = prog->getRelation("section_complete");
    auto *extraInfoTable = module.getAuxData<gtirb::schema::ElfSectionProperties>();
    if(!extraInfoTable)
        throw std::logic_error("missing elfSectionProperties AuxData table");

    for(auto &section : module.sections())
    {
        assert(section.getAddress() && "Section has no address.");
        assert(section.getSize() && "Section has non-calculable size.");

        souffle::tuple t(rel);
        auto found = extraInfoTable->find(section.getUUID());
        if(found == extraInfoTable->end())
            throw std::logic_error("Section " + section.getName()
                                   + " missing from elfSectionProperties AuxData table");
        const SectionProperties &extraInfo = found->second;
        t << section.getName() << *section.getSize() << *section.getAddress()
          << std::get<0>(extraInfo) << std::get<1>(extraInfo);
        rel->insert(t);
    }
}

souffle::SouffleProgram *DlDecoder::decode(const gtirb::Module &module,
                                           const std::vector<std::string> &DisasmOptions)
{
    assert(module.getSize() && "Module has non-calculable size.");
    gtirb::Addr minAddr = *module.getAddress();

    assert(module.getAddress() && "Module has non-addressable section data.");
    gtirb::Addr maxAddr = *module.getAddress() + *module.getSize();

    for(auto &section : module.sections())
    {
        bool Exec = section.isFlagSet(gtirb::SectionFlag::Executable);
        bool Init = section.isFlagSet(gtirb::SectionFlag::Initialized);
        if(Exec)
        {
            for(auto &byteInterval : section.byte_intervals())
            {
                decodeSection(byteInterval);
                storeDataSection(byteInterval, minAddr, maxAddr);
            }
        }
        else if(Init)
        {
            for(auto &byteInterval : section.byte_intervals())
            {
                storeDataSection(byteInterval, minAddr, maxAddr);
            }
        }
    }
    souffle::SouffleProgram *prog;
    switch(module.getISA())
    {
        case gtirb::ISA::X64:
            prog = souffle::ProgramFactory::newInstance("souffle_disasm_x64");
            break;
        case gtirb::ISA::ARM:
            prog = souffle::ProgramFactory::newInstance("souffle_disasm_arm32");
            break;
        default:
            throw std::runtime_error("Unsupported ISA ");
    }
    if(prog)
    {
        loadInputs(prog, module);
        GtirbToDatalog::addToRelation<std::vector<std::string>>(prog, "option", DisasmOptions);
        return prog;
    }
    return nullptr;
}

void DlDecoder::storeDataSection(const gtirb::ByteInterval &byteInterval, gtirb::Addr min_address,
                                 gtirb::Addr max_address)
{
    assert(byteInterval.getAddress() && "Failed to store section without address.");

    auto can_be_address = [min_address, max_address](gtirb::Addr num) {
        return ((num >= min_address) && (num <= max_address));
    };
    unsigned int PointerSize = getISAPointerSize();
    gtirb::Addr ea = byteInterval.getAddress().value();
    uint64_t size = byteInterval.getInitializedSize();
    auto buf = byteInterval.rawBytes<const uint8_t>();
    while(size > 0)
    {
        // store the byte
        uint8_t content_byte = *buf;
        data_bytes.push_back({ea, content_byte});

        // store the address
        if(size >= PointerSize)
        {
            gtirb::Addr content;
            if(PointerSize == 8)
                content = gtirb::Addr(*((uint64_t *)buf));
            else if(PointerSize == 4)
                content = gtirb::Addr(*((uint32_t *)buf));
            if(can_be_address(content))
                data_addresses.push_back({ea, content});
        }
        ++ea;
        ++buf;
        --size;
    }
}

void DlDecoder::loadInputs(souffle::SouffleProgram *prog, const gtirb::Module &module)
{
    GtirbToDatalog::addToRelation<std::vector<std::string>>(
        prog, "binary_type", *module.getAuxData<gtirb::schema::BinaryType>());
    GtirbToDatalog::addToRelation<std::vector<std::string>>(
        prog, "binary_format", {getFileFormatString(module.getFileFormat())});

    if(const gtirb::CodeBlock *block = module.getEntryPoint())
    {
        if(std::optional<gtirb::Addr> address = block->getAddress())
        {
            GtirbToDatalog::addToRelation<std::vector<gtirb::Addr>>(prog, "entry_point",
                                                                    {*address});
        }
    }
    GtirbToDatalog::addToRelation<std::vector<gtirb::Addr>>(prog, "base_address",
                                                            {module.getPreferredAddr()});

    if(auto *Relocations = module.getAuxData<gtirb::schema::Relocations>())
    {
        GtirbToDatalog::addToRelation(prog, "relocation", *Relocations);
    }

    if(auto *DataDirectories = module.getAuxData<gtirb::schema::DataDirectories>())
    {
        GtirbToDatalog::addToRelation(prog, "data_directory", *DataDirectories);
    }

    if(auto *ImportEntries = module.getAuxData<gtirb::schema::ImportEntries>())
    {
        GtirbToDatalog::addToRelation(prog, "import_entry", *ImportEntries);
    }

    GtirbToDatalog::addToRelation<std::vector<std::string>>(prog, "binary_isa",
                                                            {getISAString(module.getISA())});

    GtirbToDatalog::addToRelation(prog, "instruction_complete", instructions);
    GtirbToDatalog::addToRelation(prog, "address_in_data", data_addresses);
    GtirbToDatalog::addToRelation(prog, "data_byte", data_bytes);
    GtirbToDatalog::addToRelation(prog, "invalid_op_code", invalids);
    GtirbToDatalog::addToRelation(prog, "op_regdirect", op_dict.regTable);
    GtirbToDatalog::addToRelation(prog, "op_immediate", op_dict.immTable);
    GtirbToDatalog::addToRelation(prog, "op_indirect", op_dict.indirectTable);
    GtirbToDatalog::addToRelation(prog, "op_prefetch", op_dict.prefetchTable);
    GtirbToDatalog::addToRelation(prog, "op_barrier", op_dict.barrierTable);

    addSymbols(prog, module);
    addSections(prog, module);

    ExceptionDecoder excDecoder(module);
    excDecoder.addExceptionInformation(prog);
}