model Prueba
	Real b,c,d,e,a;
	parameter Real p = 10;
	String hola ;
	Integer m[4,5,6];
	Boolean f(y = 0, start= true) = false;
	Boolean j;
	parameter Integer pp;
	parameter Boolean tt;
	
	function sin
		input Real a;
		output Real b;
		external "c_sin";
	end sin;	
	
equation
	for i in {1,2,3} loop
		a = m[i] < 20;
	end for;	
end Prueba;     
