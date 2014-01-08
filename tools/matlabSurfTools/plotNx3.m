function plotNx3(x,opts)
  if(nargin < 2)
    opts = 'b.';
  end
  
  if(iscell(opts))
    plot3(x(:,1),x(:,2),x(:,3),opts{:});
  else
    plot3(x(:,1),x(:,2),x(:,3),opts);
  end
return