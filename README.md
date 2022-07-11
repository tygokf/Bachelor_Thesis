# Bachelor Thesis
## How to Install:
1. Download and extract OCLint source code from: https://github.com/oclint/oclint/archive/refs/tags/v22.02.tar.gz
2. Download and extract this repository
3. Copy the contents of the ```oclint-modifications``` folder in this repository
4. Paste the contents into the extracted folder from step 1 and replace the files in the destination 

## How to Build:
1. Have ```oclint-script``` as working directory
2. run ```./make```

## How to Configure:
1. Create a folder named ```etc``` inside ```oclint-22.02/build/oclint-release/```
2. Create a file named ```oclint``` inside ```etc```
3. Add configurations

## How to run locally:
1. Run ```./oclint-22.02/build/oclint-release/bin/oclint-22.02```

## Extra Files
```cg-oclint-linter``` is the wrapper script that is used to run OCLint on CodeGrade
```oclint``` is a sample configuration file that enables all the rules mentioned in this Project.
