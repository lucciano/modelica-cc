model Prueba
	Real b,c,d,e;
	Real p = 10;
	String hola;
	Integer m[4];
	Boolean f;
	Boolean j;
equation
	b =  if  (if ( b < 50) then true else false ) then 10 else 50;	
	when sample(0,1) then
		a = 1;
	elsewhen initial() then 
		a = 5;
	end when;
public
    Real c;   
end Prueba;     
