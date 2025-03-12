r:
	DB 0x80
	DB 0x40
	DB 0x20
	DB 0x10
;r = { 0x80, 0x40, 0x20, 0x10 };
l:
	DB 0x20
	DB 0x40
	DB 0x80
	DB 0x10
;l = { 0x20, 0x40, 0x80, 0x10 };
;
main:
;main()
	LD VE,0x10
;{
	LD V0,0x00
	LD V1,0x40
	LD V2,0x04
;    auto x = 0, xmax = 64, dx = 4;
	LD V3,0x00
	LD V4,0x20
	LD V5,0x04
;    auto y = 0, ymax = 32, dy = 4;
WHILE6:
	LD V6,V3
	LD V7,V4
	SUB V6,V7
	LD V6,VF
	LD VF,0x01
	XOR V6,VF
	SNE V6,0x00
	JP END6
;    while(y < ymax)
;    {
	RND VF,0xFF
	LD V6,VF
	LD V7,0x01
	AND V6,V7
	SNE V6,0x00
	JP ELSE9
;        if(rand() & 0x1)
;        {
	LD V6,V0
	LD V7,V3
	LD I,r
	DRW V6,V7,0x4
	LD V6,VF
;            draw(x, y, r);
;        }
	JP END9
ELSE9:
;        else
;        {
	LD V6,V0
	LD V7,V3
	LD I,l
	DRW V6,V7,0x4
	LD V6,VF
;            draw(x, y, l);
;        }
END9:
	LD V6,V0
	LD V7,V2
	ADD V6,V7
	LD V0,V6
	LD V7,V1
	LD VF,0x01
	SE V6,V7
	LD VF,0x00
	LD V6,VF
	SNE V6,0x00
	JP ELSE17
;        if((x += dx) == xmax)
;        {
	LD V6,V0
	LD V7,0x00
	LD V6,V7
	LD V0,V6
;            x = 0;
	LD V6,V3
	LD V7,V5
	ADD V6,V7
	LD V3,V6
;            y += dy;
;        }
	JP END17
ELSE17:
END17:
;    }
	JP WHILE6
END6:
WHILE26:
	LD V6,0x01
	SNE V6,0x00
	JP END26
;    while(1)
;    {
;        // Never leave main.
;    }
	JP WHILE26
END26:
	LD VF,0x03
	SUB VE,VF
	LD VF,V0
	LD F,VE
	LD VE,[I]
	RET
;stdio: Standard Input and Output library
getchar:
	LD V0,0xFF
	LD V1,0x00
	SKNP V1
	LD V0,0x00
	LD V1,0x01
	SKNP V1
	LD V0,0x01
	LD V1,0x02
	SKNP V1
	LD V0,0x02
	LD V1,0x03
	SKNP V1
	LD V0,0x03
	LD V1,0x04
	SKNP V1
	LD V0,0x04
	LD V1,0x05
	SKNP V1
	LD V0,0x05
	LD V1,0x06
	SKNP V1
	LD V0,0x06
	LD V1,0x07
	SKNP V1
	LD V0,0x07
	LD V1,0x08
	SKNP V1
	LD V0,0x08
	LD V1,0x09
	SKNP V1
	LD V0,0x09
	LD V1,0x0A
	SKNP V1
	LD V0,0x0A
	LD V1,0x0B
	SKNP V1
	LD V0,0x0B
	LD V1,0x0C
	SKNP V1
	LD V0,0x0C
	LD V1,0x0D
	SKNP V1
	LD V0,0x0D
	LD V1,0x0E
	SKNP V1
	LD V0,0x0E
	LD V1,0x0F
	SKNP V1
	LD V0,0x0F
	LD VF,0x03
	SUB VE,VF
	LD VF,V0
	LD F,VE
	LD VE,[I]
	RET
putchar:
	LD V5,V2
	LD V4,V1
	LD V3,V0
	LD V6,0x00
	LD B,V5
	LD V2,[I]
	LD F,V0
	DRW V3,V4,0x5
	OR V6,VF
	LD F,V1
	ADD V3,0x05
	DRW V3,V4,0x5
	OR V6,VF
	LD F,V2
	ADD V3,0x05
	DRW V3,V4,0x5
	OR V6,VF
	LD VF,0x03
	SUB VE,VF
	LD VF,V6
	LD F,VE
	LD VE,[I]
	RET
