% Oliver Hinds <oph@bu.edu>
% 2004-08-25

function examineOrientation(slices,dim)
 
  for(sliceNum=1:length(slices))
    sli = slices{sliceNum}{1};
    sli(:,dim) = [];
    
    figure,set(gcf,'doublebuffer','on'),hold on;
    for(i=1:length(sli)-1)
      line(sli(i:i+1,1), sli(i:i+1,2));
      pause(0.01);
    end
  end
  
return;
