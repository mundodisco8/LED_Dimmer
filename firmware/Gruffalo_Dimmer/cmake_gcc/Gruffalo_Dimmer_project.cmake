#eyJzdGF0ZSI6eyJidWlsZFByZXNldHMiOnsiZGVmYXVsdF9jb25maWciOnsibmFtZSI6ImRlZmF1bHRfY29uZmlnIiwiY29uZmlndXJhdGlvbiI6ImJhc2UiLCJkZWZpbml0aW9ucyI6W3sia2V5IjoiU0xfUE9XRVJfTUFOQUdFUl9ERUJVRyIsInZhbCI6IjEifSx7ImtleSI6Ik5WTTNfTUFYX09CSkVDVF9TSVpFIiwidmFsIjoiMjA0In1dLCJyZW1vdmVkRmxhZ3MiOnsiQyI6WyItT3MiLCItZyJdLCJDWFgiOltdLCJBU00iOltdfSwiYWRkaXRpb25hbEZsYWdzIjp7IkMiOlsiLU9nIiwiLWdnZGIzIl0sIkNYWCI6W10sIkFTTSI6W119fX0sImN1cnJlbnRDb25mSW5kZXgiOjB9LCJmb2xkZXJzIjpbImluYyIsInNyYyJdLCJmaWxlcyI6W119
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
	"../src/LEDIndicator.c"
	"../src/nvm_HW.c"
	"../src/PWMControl.c"
	"../src/sleepyTimers_HW.c"
	"../src/timer_HW.c"
)

add_compile_definitions(
	$<$<CONFIG:base>:SL_POWER_MANAGER_DEBUG=1>
	$<$<CONFIG:base>:NVM3_MAX_OBJECT_SIZE=204>
)

target_compile_options(slc PRIVATE
	"$<$<AND:$<CONFIG:base>,$<COMPILE_LANGUAGE:C>>:-Og>"
	"$<$<AND:$<CONFIG:base>,$<COMPILE_LANGUAGE:C>>:-ggdb3>"
)
