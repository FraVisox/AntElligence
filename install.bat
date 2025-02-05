pyinstaller ./src/engine.py --name AntElligenceEngine --noconsole --onefile
del /dist/engine_cpp/engine.dll
copy /src/engine_cpp/engine.dll ./dist/engine_cpp/engine.dll