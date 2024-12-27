
#https://docs.python.org/3.5/library/ctypes.html#fundamental-data-types
# to compile g++ -fPIC -shared -o engine.so engine.cpp

import ctypes
import pathlib
path=pathlib.Path().absolute().joinpath("engine.so")
Cengine=ctypes.CDLL(path)


# how to call a function: Cengine._[outputType]functionName[inputType](params)
Cengine._Z9startGamePc(b"ciao")
