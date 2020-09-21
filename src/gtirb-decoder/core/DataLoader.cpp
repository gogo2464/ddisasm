//===- DataLoader.cpp -------------------------------------------*- C++ -*-===//
//
//  Copyright (C) 2020 GrammaTech, Inc.
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
#include "DataLoader.h"

#include "../../AuxDataSchema.h"

void DataLoader::operator()(const gtirb::Module& Module, DatalogProgram& Program)
{
    DataFacts Facts;
    load(Module, Facts);

    Program.insert("data_byte", std::move(Facts.Bytes));
    Program.insert("address_in_data", std::move(Facts.Addresses));
}

void DataLoader::load(const gtirb::Module& Module, DataFacts& Facts)
{
    assert(Module.getAddress() && "Module has non-addressable section data.");
    Facts.Min = *Module.getAddress();

    assert(Module.getSize() && "Module has non-calculable size.");
    Facts.Max = *Module.getAddress() + *Module.getSize();

    for(const auto& Section : Module.sections())
    {
        bool Executable = Section.isFlagSet(gtirb::SectionFlag::Executable);
        bool Initialized = Section.isFlagSet(gtirb::SectionFlag::Initialized);

        if(Executable || Initialized)
        {
            for(const auto& ByteInterval : Section.byte_intervals())
            {
                load(ByteInterval, Facts);
            }
        }
    }
}

void DataLoader::load(const gtirb::ByteInterval& ByteInterval, DataFacts& Facts)
{
    assert(ByteInterval.getAddress() && "ByteInterval is non-addressable.");

    gtirb::Addr Addr = *ByteInterval.getAddress();
    uint64_t Size = ByteInterval.getInitializedSize();
    auto Data = ByteInterval.rawBytes<const int8_t>();

    while(Size > 0)
    {
        // Single byte.
        int8_t Byte = *Data;
        Facts.Bytes.push_back({Addr, Byte});

        // Possible address.
        if(Size >= static_cast<uint64_t>(PointerSize))
        {
            gtirb::Addr Value;

            switch(PointerSize)
            {
                case Pointer::DWORD:
                    Value = gtirb::Addr(*((int32_t*)Data));
                    break;
                case Pointer::QWORD:
                    Value = gtirb::Addr(*((int64_t*)Data));
                    break;
            }

            if((Value >= Facts.Min) && (Value <= Facts.Max))
            {
                Facts.Addresses.push_back({Addr, Value});
            }
        }

        ++Addr;
        ++Data;
        --Size;
    }
}
