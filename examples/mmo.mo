model Prueba
	Real b,c,d,e,a;
	parameter Real p = 10;
	String hola ;
	Integer m[4,5,6];
	Boolean f(y = 0, start= true) = false;
	Boolean j;
	parameter Integer pp;
	parameter Boolean tt;
equation
	j = if b < 50 then 1 else 5;
	for i in {1,2,3} loop
		m[i] = if tt then 1 else 2;
		m[i] = i > 4;
	end	for;

algorithm
   a := 1;

end Prueba;     
