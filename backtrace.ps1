

$a = Get-Content .\backtrace.txt -Delimiter " "
$ENV="az-delivery-devkit-v4"
$PROJECT="firmware"

foreach ( $ADDRESS in $a )
{
    ~/.platformio/packages/toolchain-xtensa32/bin/xtensa-esp32-elf-addr2line -pfiaC -e .pio/build/$ENV/$PROJECT.elf $ADDRESS
}

