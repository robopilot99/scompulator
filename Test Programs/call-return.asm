; CALL / RETURN test

		LOAD A
		CALL Math
		End:	JUMP End
		
Math:	ADDI 1
		JZERO Skip
		CALL MATH
Skip:	RETURN
		

A: DW -3