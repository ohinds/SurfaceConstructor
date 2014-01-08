% h = sliceViewer(slices,dim,anim,slice_num,box)
% sliceViewer views a set of slices
% Oliver Hinds <oph@bu.edu>
% 2004-08-05

function h = sliceViewer3d(slices,contour_closure,adjIndices,exagerate,mini,maxi)
  if(nargin < 2)
    fprintf('usage: sliceViewer3d(slices,contour_closure[,adjIndices][,exagerate,mini,maxi])\n');
    return;
  end
  if(nargin < 3)
    adjIndices = [];
  end
  if(nargin < 4)
    exagerate = 1;
  end
  if(nargin < 5)
    mini = 1;
  end
  if(nargin < 6)
    maxi = length(slices);
  end
    
  h = figure;set(gcf,'doublebuffer','on');hold on;
  c = {'k','y','c','g','r','w'};
  l = {'-','-.','--',':'};
  m = {'.','x','o','p'};
  
  for(i=mini:maxi)
    for(j=1:length(slices{i}))
      if(isempty(slices{i}{j}))
	continue;
      end
      
      curContour = slices{i}{j};
      curContour(:,3) = exagerate*curContour(:,3);
      if(contour_closure{i}(j))
	curContour(end+1,:) = curContour(1,:);
      end
      
      % plot the contour
      plotstr = sprintf('%s%s',c{mod(j-1,length(c))+1},l{floor(j/length(c))+1});
      %plotstr = sprintf('%s%s',c{mod(j-1,length(c))+1},m{floor(j/length(c))+1});
      plotNx3(curContour,plotstr);
      
      % plot adjacency
      if(isempty(adjIndices))
	continue;
      end
      
      if(i < length(slices))
	adj = adjIndices{i}{j};
	for(k=1:length(adj)-1)
	  lin = [curContour(1,:);
		 [slices{i+1}{adj(k)+1}(1,1:2) ...
		  exagerate*slices{i+1}{adj(k)+1}(1,3)]];
	  plotNx3(lin,'b-');
	end
      end
    end
  end
  
  axis equal;
  cameratoolbar
return

