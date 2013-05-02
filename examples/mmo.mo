model Prueba
	Real b,c,d,e;
	String hola;
	Integer m;
	Boolean f;
	Boolean j;
equation
	b = (if f and j then 50 else 30) + e;
	f = 10 < 5 and j;
public
    Real c;   
end Prueba;     
