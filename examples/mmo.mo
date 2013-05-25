model Prueba
	Real b,c,d,e;
	parameter Real p = 10;
	String hola;
	Integer m[4,5,6];
	Boolean f;
	Boolean j;
equation
	if f then 
		a = 1;
	else
		a = 2;
	end if;
	for i in 1:10 loop
		m[i] = if i < 4 then 1 else 2;
		m[i] = i > 4;
	end	for;
	
	a = 1 < 10;
	(if 1 < 10 then p else e ) = b;
	
algorithm
   a := 1;

end Prueba;     
