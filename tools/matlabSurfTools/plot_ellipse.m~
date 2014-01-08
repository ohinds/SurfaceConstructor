% plot_ellipse
%
% Oliver Hinds <oph@bu.edu>
% 2005-05-23

function h = plot_ellipse(a,b,tx,ty,r,opts)

  if(nargin < 6)
    opts = 'b';
  end

  step = 0.1;

  x = [-a:step:a]';
  y = [sqrt(b^2*(1-x.^2./a^2));flipud(-sqrt(b^2*(1-x.^2./a^2)))];

  xy = [[x;flipud(x)],y];
  for(i=1:size(xy,1))
    xy(i,:) = ([cos(r) sin(r); -sin(r) cos(r)]*xy(i,:)' + [tx ty]')';
  end

  h = plot(xy(:,1), xy(:,2), opts);
  axis('equal');

return
