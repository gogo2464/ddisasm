import os
import platform
import unittest
from disassemble_reassemble_check import disassemble_reassemble_test as dis_reasm_test
from disassemble_reassemble_check import skip_reassemble, reassemble_using_makefile
from disassemble_reassemble_check import compile,cd

windows_optimization_levels = ['/Od', '/Ot', '/O1', '/Ox', '/O2']


class TestSmallStripWindows(unittest.TestCase):
    # Windows tests
    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_1_win(self):
        self.assertTrue(dis_reasm_test('examples\ex1', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_2modulesPIC_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_2modulesPIC', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_confusing_data_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_confusing_data', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_false_pointer_array_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_false_pointer_array', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_float_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_float', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_fprintf_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_fprintf', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_noreturn_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_noreturn', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_pointerReatribution_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_pointerReatribution', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_pointerReatribution2_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_pointerReatribution2', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_pointerReatribution3_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_pointerReatribution3', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))
        
    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_struct_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_struct', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_switch_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_switch', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))
        
    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_uninitialized_data_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_uninitialized_data', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_legacy_switch_001_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_legacy_switch.001', 'main.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_legacy_switch_002_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_legacy_switch.002', 'main.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_legacy_switch_003_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_legacy_switch.003', 'main.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_legacy_switch_004_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_legacy_switch.004', 'main.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], ['/Od', '/Ot', '/O1', '/Ox']))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_memberPointer_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_memberPointer', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_virtualDispatch_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_virtualDispatch', 'ex.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], windows_optimization_levels))

    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_simple_dll_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_simple_dll', 'test.dll', [], [], 'ml64', ['cl'], ['cl'], windows_optimization_levels, False, reassemble_using_makefile))

    # Windows examples which fail to reassemble
    @unittest.skipUnless(platform.system() == 'Windows', 'This test is windows only.')
    def test_legacy_switch_004_O2_win(self):
        self.assertTrue(dis_reasm_test('examples\ex_legacy_switch.004', 'main.exe', [], ['/link', '/subsystem:console', '/entry:__EntryPoint'], 'ml64', ['cl'], ['cl'], ['/O2'], reassemble_function=skip_reassemble))    