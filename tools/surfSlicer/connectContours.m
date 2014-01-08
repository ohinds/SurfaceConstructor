function sl = connectContours(sl,sinds,c1,c2,cl)
  for(s=1:length(sinds))
    sind = sinds(s);
     con = [
	(sl{sind}{c1}(1,:)-sl{sind}{c2}(1,:)).^2
	(sl{sind}{c1}(1,:)-sl{sind}{c2}(end,:)).^2
	(sl{sind}{c1}(end,:)-sl{sind}{c2}(1,:)).^2
	(sl{sind}{c1}(end,:)-sl{sind}{c2}(end,:)).^2
	  ];  
    [trash mind] = min(sum(con,2));
    switch(mind)
     case 1
      sl{sind}{c1} = vertcat(flipud(sl{sind}{c1}), sl{sind}{c2});
     case 2
      sl{sind}{c1} = vertcat(sl{sind}{c2},sl{sind}{c1});
     case 3
      sl{sind}{c1} = vertcat(sl{sind}{c1},sl{sind}{c2});
     case 4
      sl{sind}{c1} = vertcat(sl{sind}{c1}, flipud(sl{sind}{c2}));
    end    
    sl{sind}(c2) = [];
    cl{sind}(c2) = [];
  end
return