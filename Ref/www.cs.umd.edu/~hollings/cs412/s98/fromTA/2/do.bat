@rem - Heading -
@echo.
@echo                          T E S T   %1
@echo.
@set oldprompt=%prompt%
@prompt :
@rem - Prepare new sources -
@copy test%1.c proc.c
@echo Copied the test source. Will make the project now.
@pause
@rem - Create new executable -
@if exist proc.obj del proc.obj
@if exist proj.exe del proj2.exe
@make > MAKE
@rem - Clean up & output -
@prompt %oldprompt%
@type MAKE | more
@rem - Go! -
@rem echo.
@rem - NT does not know choice - @choice /c:yn "           Will test now "
@rem - NT does not know choice - @if errorLevel 2 goto eof
@echo.
@echo		WILL TEST NOW
@echo.
@pause
@rem - NT can "start" in a separate window
@rem - Pure DOS would "call"
@start proj2.exe
:eof
