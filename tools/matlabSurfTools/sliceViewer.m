% h = sliceViewer(slices,dim,anim,slice_num,box)
% sliceViewer views a set of slices
% Oliver Hinds <oph@bu.edu>
% 2004-08-05

function h = sliceViewer(slices,dim,anim,slice_num,box)
  global colors

  showBox = 0;
  
  if(nargin < 2)
    dim = 3;
  end
  if(nargin < 3)
    anim = 1;
  end
  if(nargin < 4)
    slice_num = -1;
  end
  if(nargin > 5)
    showBox = 1;;
  end

  big = 100000000;

  % display params
  frameRate = 5;
  colors = ['b' 'r' 'g' 'k' 'm' 'y'];
  m = {'.','x','o','p'};


  % open a figure
  h = figure,set(gcf,'doublebuffer','on');

  axisrange = [big -big big -big];
  for(i=1:length(slices))
    for(j=1:length(slices{i}))
      curContour = slices{i}{j};
      curContour(:,dim) = [];

      if(min(curContour(:,1)) < axisrange(1))
	axisrange(1) = min(curContour(:,1));
      end
      if(max(curContour(:,1)) > axisrange(2))
	axisrange(2) = max(curContour(:,1));
      end
      if(min(curContour(:,2)) < axisrange(3))
	axisrange(3) = min(curContour(:,2));
      end
      if(max(curContour(:,2)) > axisrange(4))
	axisrange(4) = max(curContour(:,2));
      end
    end
  end
  axisrange

  % check if only one slice should be shown
  if(slice_num > 0)
    showslice(slices,slice_num,dim,axisrange);

    if(showBox)
      hold on;
      plot([box(1) box(1) box(2) box(2) box(1)], [box(3) box(4) ...
		    box(4) box(3) box(3)], 'k--');
      hold off;
    end
    
    return;
  end

  % plot each slice
  fr = 1;
  while(1)
    showslice(slices,fr,dim,axisrange);

    if(showBox)
      hold on;
      plot([box(1) box(1) box(2) box(2) box(1)], [box(3) box(4) ...
		    box(4) box(3) box(3)], 'k--');
      hold off;
    end
    
    if(anim)
      pause(1/frameRate);
    else
      pause;
    end

    fr = mod(fr,size(slices,2)-1)+1;
  end
return

function showslice(slices,n,dim,axisrange)
  global colors
  
  curSlice = slices{n};
  curDist = curSlice{1}(1,dim);

  for(j=1:length(curSlice))
    curContour = curSlice{j};
    curContour(:,dim) = [];

    if(j > 1)
      hold on;
    else
      hold off;
    end

    plot(curContour(:,1),curContour(:,2),[colors(mod(j,length(colors))+1) '.-']);
    axis(axisrange);
    axis equal;
  end

  %text(axisrange(1),axisrange(3),num2str(n));
  text(axisrange(1),axisrange(3),num2str(length(slices{n})));
return
