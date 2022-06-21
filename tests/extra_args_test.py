import platform
import tempfile
import unittest
from disassemble_reassemble_check import (
    compile,
    cd,
    disassemble,
)
from pathlib import Path
import gtirb


ex_dir = Path("./examples/")
ex_asm_dir = ex_dir / "asm_examples"


class ExtraArgsTest(unittest.TestCase):
    @unittest.skipUnless(
        platform.system() == "Linux", "This test is linux only."
    )
    def test_user_hints(self):
        """Test `--hints'. The test disassembles, checks
        the address of main and disassembles again with
        a hint that makes main invalid. After the second
        disassembly, main is not considered code.
        """

        with cd(ex_dir / "ex1"):
            # build
            self.assertTrue(compile("gcc", "g++", "-O0", []))

            # disassemble
            self.assertTrue(
                disassemble(
                    "ex",
                    format="--ir",
                )[0]
            )

            # load the gtirb
            ir = gtirb.IR.load_protobuf("ex.gtirb")
            m = ir.modules[0]

            main_sym = next(sym for sym in m.symbols if sym.name == "main")
            main_block = main_sym.referent
            self.assertIsInstance(main_block, gtirb.CodeBlock)
            # dissasemble with hints
            with tempfile.TemporaryDirectory() as debug_dir:
                with tempfile.NamedTemporaryFile(mode="w") as hints_file:
                    print(
                        f"invalid\t{main_block.address}\tuser-provided-hint",
                        file=hints_file,
                        flush=True,
                    )
                    self.assertTrue(
                        disassemble(
                            "ex",
                            format="--ir",
                            extra_args=[
                                "--debug-dir",
                                debug_dir,
                                "--hints",
                                hints_file.name,
                            ],
                        )[0]
                    )

                # load the new gtirb
                ir = gtirb.IR.load_protobuf("ex.gtirb")
                m = ir.modules[0]

                main_sym = next(sym for sym in m.symbols if sym.name == "main")
                # main cannot be code if we tell it explicitly that
                # it contains an invalid instruction through hints
                main_block = main_sym.referent
                self.assertIsInstance(main_block, gtirb.DataBlock)
                self.assertIn(
                    "user-provided-hint",
                    (Path(debug_dir) / "invalid.csv").read_text(),
                )


if __name__ == "__main__":
    unittest.main()
