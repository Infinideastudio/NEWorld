echo Installing dependiencies...
:: Install Boost libraries
echo Installing Boost libraries...
mkdir boost
cd boost\
nuget install boost_system-vc140 -Version 1.61.0 > nul
nuget install boost_filesystem-vc140 -Version 1.61.0 > nul
nuget install boost_date_time-vc140 -Version 1.61.0 > nul
nuget install boost_regex-vc140 -Version 1.61.0 > nul
cd ..
:: Install other dependencies
echo Installing other dependencies...
appveyor DownloadFile https://raw.githubusercontent.com/Infinideastudio/NEWorld-binaries/master/dependencies.exe
C:\NEWorld\dependencies.exe > nul
:: Link
echo Creating links...
mklink /D dependencies\include\boost C:\NEWorld\boost\boost.1.61.0.0\lib\native\include\boost
mkdir dependencies\lib\boostlib
copy C:\NEWorld\boost\boost_filesystem-vc140.1.61.0.0\lib\native\address-model-32\lib\libboost_filesystem-vc140-mt-gd-1_61.lib dependencies\lib\boostlib
copy C:\NEWorld\boost\boost_system-vc140.1.61.0.0\lib\native\address-model-32\lib\libboost_system-vc140-mt-gd-1_61.lib dependencies\lib\boostlib
copy C:\NEWorld\boost\boost_date_time-vc140.1.61.0.0\lib\native\address-model-32\lib\libboost_date_time-vc140-mt-gd-1_61.lib dependencies\lib\boostlib
copy C:\NEWorld\boost\boost_regex-vc140.1.61.0.0\lib\native\address-model-32\lib\libboost_regex-vc140-mt-gd-1_61.lib dependencies\lib\boostlib
