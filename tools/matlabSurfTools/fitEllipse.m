% fit_ellipse fits an ellipse to the boundary of a flat surface
%
% [A, B, TX, TY, R, S] = FIT_ELLIPSE(FLATSURF[,FIT_LABELS])
% A  is the semimajor axis
% B  is the semiminor axis
% TX is translation laterally
% TY is translation vertically
% R  is rotation
% S  is scale
% 
% Oliver Hinds <oph@bu.edu>
% 2005-05-04

function ellipse = fitEllipse(surfStruct,r0,a0,b0,tx0,ty0)

  if(nargin == 2)
    surfStruct = extractpatchCC(extractpatchQ(surfStruct,find(surfStruct.vertexLabels==1)));
    r0 = [];
  end
  
  
  weight = 0;
  upsample = 0;
  
  if(~isstruct(surfStruct))
    coords = surfStruct;
    [v,e] = eig(cov(coords));
  else
    if(upsample)
      coords = upsampleBorder(surfStruct);
    else
      coords = surfStruct.vertices(boundaryVertices(surfStruct),:);
    end
    [v,e] = eig(cov(surfStruct.vertices(:,1:2)));
  end
  
  % do an eigenanalysis to find best initial parameters
  [v,e] = eig(cov(coords));
  e = diag(e);
  [trash, mxind] = max(e);
  
%   mn = min(coords);
%   mx = max(coords);
%   ord = sort((mx-mn)/3,1,'descend');
  ord = sort(sqrt(e),1,'descend');

  if(nargin < 2 | isempty(r0))
    r0 = atan2(v(mxind,2),v(mxind,1));
    if(r0 < pi/2)
      r0 = r0 + pi;
    elseif(r0 > 3*pi/2)
      r0 = r0 - pi;
    end
  end  

  if(nargin < 3 | a0 == [])
    a0 = ord(1);
  end

  if(nargin < 4 | b0 == [])
    b0 = ord(2);
  end

  if(nargin < 5 | tx0 == [])
    tx0 = mean(coords(:,1));
  end

  if(nargin < 6 | ty0 == [])
    ty0 = mean(coords(:,2));
  end
  
  
  % construct vectors for parameters
  x0 = [a0,b0,tx0,ty0,r0]; % initial model parameters

  if(weight)
    [wgrid, wmapx, wmapy] = get_wgrid(coords,2);
  end
  
%   figure,set(gcf,'doublebuffer','on');  
%   plot(coords(:,1), coords(:,2),'.');
%   hold on
%   plot_ellipse(a0,b0,tx0,ty0,r0,'r');

  opt = optimset('display','off');
  if(weight)
    [x,fval,flag,out] = fminsearch(@ellipseErr,x0,opt,coords,wgrid,wmapx,wmapy);
    [res] = ellipseErr(x,coords,wgrid,wmapx,wmapy);
  else
    [x,fval,flag,out] = fminsearch(@ellipseErr,x0,opt,coords);
    [res] = ellipseErr(x,coords);
  end
  
  % check the error flag
  if(flag < 0) 
    fprintf('error: fminsearch did not converge\n');
  elseif(flag == 0) 
    fprintf('error: fminsearch exceeded the max number of iterations\n');
  else % if success, print some info
    fprintf(['success: the %s algorithm performed %d function' ...
	    ' evaluations using %d iterations\n'], out.algorithm, ...
	    out.funcCount, out.iterations);
  end
  
  ellipse.a = x(1);
  ellipse.b = x(2);
  ellipse.tx = x(3);
  ellipse.ty = x(4);
  ellipse.r = x(5);
  ellipse.e = res;

return;

