# Deben, a simple accounting tool.

## Commands 

- `--p,--path <path to config>`  
    Path to the file listing all operations. If not specified, Deben will look at the `DEBEN_FILE` environment variable.

### Operations

- `--a,--add <[+,-]amount 'label' dd[/mm[/YYYY]]>`  
    Add an operation (`--add` is optional). An unsigned amount is assumed to be negative. Date can be partially specified and will be completed using the current day/month/year.
- `--d,--delete <i>`  
    Remove operation at index i (the last one by default)
- `--l,--list <n>`  
    List the last n operations (40 by default)

### Modifiers
- `--nc,--no-color`  
    Do not use color ANSI modifiers in the output.

### Infos
- `--v,--version`  
    Displays the current Thoth version.
- `--license`  
    Display the license message.

## Functionalities
All operations are stored in a simple text file using the following format:

`YYYY/MM/DD amount label`

Lines beginning with a `#` will be ignored (but preserved).  

## Future improvements

- add bulk import
- add visualisation
