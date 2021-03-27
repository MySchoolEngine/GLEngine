
CALL :runClang Core
CALL :runClang Utils
CALL :runClang GLRenderer
CALL :runClang DX12Renderer
CALL :runClang Renderer
CALL :runClang Physics
CALL :runClang GUI
CALL :runClang Entity
CALL :runClang Audio

cd Data
cd Shaders
@for /R "." %%I in (*.glsl) do @clang-format -style=file -i --verbose "%%I" 
cd ..
cd ..
goto :eof

:runClang
cd %~1
@for /R %~1 %%I in (*.cpp *.h *.inl *.glsl) do @clang-format -style=file -i --verbose "%%I" 

cd ..