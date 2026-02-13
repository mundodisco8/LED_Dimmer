#eyJzdGF0ZSI6eyJidWlsZFByZXNldHMiOnsiZGVmYXVsdF9jb25maWciOnsibmFtZSI6ImRlZmF1bHRfY29uZmlnIiwiY29uZmlndXJhdGlvbiI6ImJhc2UiLCJkZWZpbml0aW9ucyI6W10sInJlbW92ZWRGbGFncyI6eyJDIjpbIi1PcyIsIi1nIl0sIkNYWCI6W10sIkFTTSI6W119LCJhZGRpdGlvbmFsRmxhZ3MiOnsiQyI6WyItT2ciLCItZ2dkYjMiXSwiQ1hYIjpbXSwiQVNNIjpbXX19fSwiY3VycmVudENvbmZJbmRleCI6MH0sImZvbGRlcnMiOlsiaW5jIiwic3JjIl0sImZpbGVzIjpbXX0=
include_directories(
	"../inc"
	"../src"
)

target_sources(Gruffalo_Dimmer PRIVATE
	"../src/brightnessLUT.c"
	"../src/buttonActions.c"
	"../src/buttons.c"
	"../src/debounce.c"
	"../src/effectControl.c"
	"../src/gpio_HW.c"
	"../src/interrupt_HW.c"
	"../src/PWMControl.c"
	"../src/sleepyTimers_HW.c"
	"../src/timer_HW.c"
)

target_compile_options(slc PRIVATE
	"$<$<AND:$<CONFIG:base>,$<COMPILE_LANGUAGE:C>>:-Og>"
	"$<$<AND:$<CONFIG:base>,$<COMPILE_LANGUAGE:C>>:-ggdb3>"
)
