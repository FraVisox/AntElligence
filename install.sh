#/bin/bash
pyinstaller ./src/engine.py --name AntElligenceEngine --add_ --noconsole --onefile
rm dist/engine_cpp/engine.dll
cp src/engine_cpp/engine.dll dist/engine_cpp/engine.dll