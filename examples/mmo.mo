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
	if tt then 
		if j then 
			a = 1;
		else 
			b = 15;
		end if;	
	else 
		c = 15;
	end if;	
algorithm
	when j then 
		tt := true;
	end when;	
end Prueba;     
