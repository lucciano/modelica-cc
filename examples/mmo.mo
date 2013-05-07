model Prueba
	Real b,c,d,e;
	String hola;
	Integer m[4];
	Boolean f;
	Boolean j;
equation
	b = (if not false and j then 50 else 30) + m[1];
		
	when b < 5 then 
		if j then a = 10; else a = 30; end if ;
	elsewhen b > 5 then 	
		a = 50;
	end when;	
public
    Real c;   
end Prueba;     
