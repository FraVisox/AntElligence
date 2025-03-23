g++ -shared -o ./src/cpp_and_py/engine_cpp/engine.dll ./src/cpp_and_py/engine_cpp/*.cpp -static -static-libgcc -static-libstdc++ -I. -O3 -Wall -DBUILDING_DLL        
pyinstaller ./src/pyth/engine.py --name AntElligenceEnginePyCpp --noconsole --onefile
del ".\dist\engine_cpp\engine.dll"
copy ".\src\cpp_and_py\engine_cpp\engine.dll" ".\dist\engine_cpp\engine.dll"