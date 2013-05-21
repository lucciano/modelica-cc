model Prueba
	Real b,c,d,e;
	Real p = 10;
	String hola;
	Integer m[4];
	Boolean f;
	Boolean j;
equation
	for i in 1:10 loop
		m[i] = if f < 4 then 1 else 2;
		m[i] = i > 4;
	end	for;
		
public
    Real c;   
end Prueba;     
