<h1>Extensible Model Representation - XMR</h1>
<h3>Project Requirements</h3>
- git >=2.43.5 <br>
- cmake >=3.25.0 <br>
- C++ >=20 <br>
<h3>Building Project</h3>
1. Configure CMake project: cmake -S ./xmr -B ./build <br>
2. Build project: cmake --build ./build 
<h3>Running The Tool</h3>
After building there will be an XMR binary in the build director. This is the application binary to invoke. <br>
There is only one required option -f which lets you specify the model meta-data input file to be parsed and generated. <br>
If no other options are specified, the tool will assume you are providing a UML XMI file from the Papyrus modeling tool <br> 
and it will generate C++ code. The code will be outputted to a.cpp in the same directory you executed the binary from. <br>
There are three optional command line arguments: <br>
-o: This lets you specify a new output file name
-g: This lets you set a new code generator shared object file to dynamically loaded at runtime to provide a different generator than the built in C++ generator. The shared object library need to provide an extern to a create_generator method that returns a new IGenerator object and a destroy_generator method that deallocates the IGenerator object. 
-p: This lets you set a new parser shared object file to dynamically loaded at runtime to provide a different parser than the build in UML XMI Papyrus parser. The shared object library needs to provide an extern to a create_parser method that returns a new IParser object and a destroy_parser method that deallocates the IParser object. 