FUNCTION(PUSH_TEST name files)
	set(TARGET hk_test_${name})
	add_executable(${TARGET} ${files})
	target_link_libraries(${TARGET} hikari ${CMAKE_DL_LIBS})
	add_test(NAME ${name} COMMAND ${TARGET})
ENDFUNCTION(PUSH_TEST)

PUSH_TEST(init test_init.cpp)