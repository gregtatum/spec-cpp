# Generate a compile_commands.json for clangd integration in the editor.
# This magic script: https://stackoverflow.com/questions/21134120/how-to-turn-makefile-into-json-compilation-database
# vscode clangd instructions: https://marketplace.visualstudio.com/items?itemName=llvm-vs-code-extensions.vscode-clangd


make --always-make --dry-run \
 | grep -wE 'clang\+\+' \
 | grep -w '\-c' \
 | jq -nR '[inputs|{directory:"'"$(pwd)"'", command:., file: match(" [^ ]+$").string[1:]}]' \
 > compile_commands.json
