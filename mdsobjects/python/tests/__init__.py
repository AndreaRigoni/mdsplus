"""
MDSplus tests
==========

Tests of MDSplus

"""
def _mimportSuite(name, level=1):
    try:
        return __import__(name, globals(), level=level).suite
    except:
        return __import__(name, globals()).suite

from unittest import TestSuite,TextTestRunner
import os,sys
import gc;gc.set_debug(gc.DEBUG_UNCOLLECTABLE)

from MDSplus import setenv,getenv
if os.name=='nt':
    setenv("PyLib","python%d%d"  % sys.version_info[0:2])
else:
    setenv("PyLib","python%d.%d" % sys.version_info[0:2])

def test_all(*arg):
    if getenv('waitdbg') is not None:
      print("Hit return after gdb is connected\n")
      sys.stdin.readline()
    testSuites = [
        'connectionUnitTest',
        'dataUnitTest',
        'dclUnitTest',
        'devicesUnitTest',
        'exceptionUnitTest',
        'segmentsUnitTest',
        'treeUnitTest',
        'threadsUnitTest',
    ]
    tests=[]
    for suite in testSuites:
        try:
            tests.append(_mimportSuite(suite)())
        except Exception as e:
            print("Could not import %s\n%s"%(suite,e.message))
    return TestSuite(tests)

def run():
    TextTestRunner().run(test_all())

if __name__=='__main__':
    run()
