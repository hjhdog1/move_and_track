rem
rem The following commands show to to build the code here from the command
rem line. The application requires that canlibCLSNET.dll is somewhere in path
rem in order to run.
rem If compilation fails, make sure CSC.exe from Microsoft.NET Framework is 
rem somwhere in the path.
rem
csc /nologo /target:winexe /resource:CSharpSimpleWindowSample.resx /reference:..\..\rel\bin\canlibclsnet.dll CSharpSimpleWindowSample.cs AssemblyInfo.cs
