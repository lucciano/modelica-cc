model Prueba
	Real b,c,d,e;
	String hola;
	Integer[3,4] m[4,6];
	Boolean f;
	Boolean j;
equation
	b = (if f and j then 50 else 30) + e;
  for i in 1:10,j in a loop
  end for;
	when b < 5 then 
		if j then a = 10; else a = 30; end if ;
	elsewhen b > 5 then 	
		a = 50;
	end when;	
public
    Real c;   
end Prueba;     
