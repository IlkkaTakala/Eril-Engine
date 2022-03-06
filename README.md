# Project Eril

For information about the project, see the links below

## Documentation

[See all documentation](https://repo.kamit.fi/ttv20sp_pelimoottorit/tiimi2/-/tree/master/docs)

* [Documentation with project members and timelines](../../tree/master/docs/ProjectPlan.md).
* [Technical Design Document](https://edukainuu-my.sharepoint.com/:w:/g/personal/ilkkatakala_kamk_fi/EcEa7uBOUQ1Gi0DgWoC1M0wB5g6iNabpp7acVAQEV1U5ZQ).
* [Demo designs](../../tree/master/docs/DemoDesign.md)
* [Scripting reference](../../tree/master/docs/ScriptDoc.md)

Work in progress documentation for newest features can be found on the "[docs](../../tree/docs/docs/)" branch.

## Building the project

* Run cmake on Engine/external/bullet3/ directory and enable "Use MSVC runtime library" option.
* Open and build the created solution  
* Copy created library files to build/lib/
* Run cmake on Engine/external/openal-soft/
* Build the solution and copy openal library to build/lib/ and OpenAL32.dll to build/(BuildConfig)/
* Run cmake on Engine/ScriptCore/
* Build the solution and copy ErilScript.lib to build/lib/ and ErilScript.dll to build/(BuildConfig)/
* Open Eril solution and rebuild solution
* Game should build correctly after that, contact ilkkatakala@kamk.fi if the steps didn't work
