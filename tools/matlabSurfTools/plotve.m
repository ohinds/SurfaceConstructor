% v is vertices
% e is edges
% n is edge normals
%
% Oliver Hinds <oph@bu.edu>
% 2005-08-22

function plotve(v,e,n,opts,h)
  if(nargin < 2)
    fprintf('usage: plotve(v,e)\n');
    return;
  end
  if(nargin < 3)
    n = [];
  end
  if(nargin < 4)
    opts = 'b';
  end
  if(nargin < 5)
    figure;
    h = gca;
  end

  hold on;
  xmin = min(v(:,1)); xmax = max(v(:,1));
  xlen = xmax - xmin;
  ymin = min(v(:,2)); ymax = max(v(:,2));
  ylen = ymax - ymin;

  xvlen = 0.1*xlen;
  yvlen = 0.1*ylen;
  
  set(gca,'xlim',[xmin-0.1*xlen xmax+0.1*xlen]);
  set(gca,'ylim',[ymin-0.1*ylen ymax+0.1*ylen]);
  
  %plot(v(:,1),v(:,2),[opts '*']);
  for(i=1:size(e,1))
    % see if this edge has no second end point
    if(e(i,2) == -1 & size(n,1) >= i)
      % find a point outside the bounds
      a = atan2(n(i,2),n(i,1));
      outpt = v(e(i,1),:) + [xlen*cos(a) ylen*sin(a)];
      pts = [v(e(i,1),:); outpt];
      plot(pts(:,1), pts(:,2), [opts '-']);
    elseif(e(i,2) ~= -1 )
      plot(v(e(i,:),1),v(e(i,:),2), [opts '-*']);
    end
  end

return
