% plot_ellipse
%
% Oliver Hinds <oph@bu.edu>
% 2005-05-23

function h = plot_ellipse(e,opts,step)

  if(nargin < 2)
    opts = {'b'};
  end
  
  if(nargin < 3)
    step = 0.1;
  end
  
  a = e.a;
  b = e.b;
  tx = e.tx;
  ty = e.ty;
  r = e.r;
  
  x = [-a:step:a]';
  y = [sqrt(b^2*(1-x.^2./a^2));flipud(-sqrt(b^2*(1-x.^2./a^2)))];

  xy = [[x;flipud(x)],y];
  for(i=1:size(xy,1))
    xy(i,:) = ([cos(r) sin(r); -sin(r) cos(r)]*xy(i,:)' + [tx ty]')';
  end

  h = plot(xy(:,1), xy(:,2), opts{:});
  axis('equal');

return
