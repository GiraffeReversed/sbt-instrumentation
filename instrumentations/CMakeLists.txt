install(FILES
	memsafety/config.json
	memsafety/config-noconst.json
	memsafety/config-basic.json
	memsafety/config-basic-lifetimes.json
	memsafety/config-enhanced.json
	memsafety/config-enhanced-noconst.json
	memsafety/config-marker.json
	memsafety/config-marker-memcleanup.json
	memsafety/config-marker-predator.json
	memsafety/config-marker-predator-pta.json
	memsafety/config-i2-enhanced.json
	memsafety/config-i2-enhanced-lifetimes.json
	memsafety/config-lifetimes.json
	memsafety/config-minmax.json
	memsafety/config-memcleanup.json
	memsafety/memsafety.c
	memsafety/marker.c
	DESTINATION ${CMAKE_INSTALL_DATADIR}/sbt-instrumentation/memsafety/
)
install(FILES
	memsafety/sttt-experiments/config-basic.json
	memsafety/sttt-experiments/config-rc-rt.json
	memsafety/sttt-experiments/config-rc.json
	memsafety/sttt-experiments/config-i1-rt.json
	memsafety/sttt-experiments/config-new-checkbounds.json
	DESTINATION ${CMAKE_INSTALL_DATADIR}/sbt-instrumentation/memsafety/sttt-experiments
)

install(FILES
	null_deref/config.json
	null_deref/null_deref.c
	DESTINATION ${CMAKE_INSTALL_DATADIR}/sbt-instrumentation/null_deref/
)

install(FILES
	termination/config.json
	termination/termination.c
	DESTINATION ${CMAKE_INSTALL_DATADIR}/sbt-instrumentation/termination/
)

install(FILES
    int_overflows/config-no-assert.json
    int_overflows/config-no-ra.json
    int_overflows/overflows.c
    int_overflows/overflows-no-assert.c
    DESTINATION ${CMAKE_INSTALL_DATADIR}/sbt-instrumentation/int_overflows/
)
