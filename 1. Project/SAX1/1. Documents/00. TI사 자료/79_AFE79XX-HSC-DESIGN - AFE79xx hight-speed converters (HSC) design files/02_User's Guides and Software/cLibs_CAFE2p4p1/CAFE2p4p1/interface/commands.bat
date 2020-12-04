g++ -std=c++11 -DMS_WIN64 -IftdiAPI/amd64 -c -o ftdi_wrapper.o ftdi_wrapper.cpp

g++ -std=c++11 -DMS_WIN64 -IftdiAPI/amd64 -c -o helperFunctions.o helperFunctions.cpp

g++ -std=c++11 -shared -static -static-libstdc++ -DMS_WIN64 ./ftdi_wrapper.o ./helperFunctions.o -lftd2xx -LftdiAPI/amd64 -o libwrapper.dll

g++ -std=c++11 -shared -static -static-libstdc++ -DMS_WIN64 -IftdiAPI/amd64  interface.cpp -lftd2xx -LftdiAPI/amd64  -L. libwrapper.dll -o libinterface.dll

