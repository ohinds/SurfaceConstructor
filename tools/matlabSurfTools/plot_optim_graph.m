% Oliver Hinds <oph@bu.edu>
% 2005-08-30

function plot_optim_graph(e,vw,hw)

% create a grid
  c0 = 1:size(vw,1); %max(e(:,1)):-1:0';
  c1 = 1:size(vw,2); %0:max(e(:,2))';
  [realC0Len realC1Len] = size(vw);

  % regularize the weights
  tmp = max(vw);
  tmp(find(tmp == Inf)) = min(tmp);
  vw(find(isnan(vw) | isinf(vw))) = max(tmp);
  cm = jet;
  vw = round(1+(size(cm,1)-1)*vw/max(max(vw)));
  % horiz
  tmp = max(hw);
  tmp(find(tmp == Inf)) = min(tmp);
  hw(find(isnan(hw) | isinf(hw))) = max(tmp);
  cm = jet;
  hw = round(1+(size(cm,1)-1)*hw/max(max(hw)));
  
%   % input contour vertices
%   c0 = [ -2,  0, 1;  % 0
% 	 -1, -1, 1;  % 1
% 	  1, -1, 1;  % 2
% 	  2,  0, 1;  % 3
% 	  0,  1, 1]; % 4
   c0markerColor = [0 0 1];
  
%   c1 = [  1, -1, 0;  % 5
% 	  1,  1, 0;  % 6
% 	 -1,  1, 0;  % 7
% 	 -1, -1, 0]; % 8
   c1markerColor = [1 0 0];
  
  contourEdgeColor = [0 0 1];
  verticalEdgeColor = [0 0 0];
  faceAlpha = 1.0;
  
  % graph edges in surface
  e = e+1;
%   e = [2 0; 
%        3 0; 
%        3 1;
%        4 1;
%        4 2;
%        5 2;
%        5 3;
%        6 3;
%        7 3]+1;
  
  numFaces = size(e,1)-1;
  edgeColors = zeros(numFaces,3);
  edgeColors(:,1) = 1;
  edgeColors(:,2) = 0.5:0.5/(numFaces-1):1;
  
  % get a new figure
  %figure;
  hold on;
  %grid on;
  
  % plot the grid
%   for(i=c0(1:end-1))
%     for(j=c1(1:end-1))
%       line([j j+1],[i-1 i-1],'linewidth',2,'color',cm(w(i,j+2),:));
%       line([j+1 j+1],[i-1 i],'linewidth',2,'color',cm(w(i+1,j+2),:));
%     end
%   end
%   line([c1(1) c1(1)],[c0(1) c0(end)],'linewidth',2,'color',verticalEdgeColor);
%   line([c1(1) c1(end)],[c0(1) c0(1)],'linewidth',2,'color',verticalEdgeColor);
  for(i=1:realC0Len)
    for(j=1:realC1Len)
      line([j-1 j],[realC0Len-(i-1) realC0Len-(i-1)],'linewidth',2,'color',cm(hw(i,j),:));
      line([j-1 j-1],[realC0Len-(i-1) realC0Len-i],'linewidth',2,'color',cm(vw(i,j),:));
    end
  end
  % run down sides
  for(i=1:realC0Len)
    line([realC1Len realC1Len],[realC0Len-(i-1) realC0Len-i],'linewidth',2,'color',cm(vw(i,realC1Len),:));
  end
  for(j=1:realC1Len)
    line([j-1 j],[0 0],'linewidth',2,'color',cm(hw(realC0Len,j),:));
  end

  % plot the edges
  for(i=1:numFaces)
%    line(c1(e(i:i+1,2)),c0(e(i:i+1,1)),'linewidth',3,'color',edgeColors(i,:));

    if(e(i,1) == e(i+1,1)) % vert
      c = cm(vw(e(i,1),e(i,2)),:);
    else
      c = cm(hw(e(i,1),e(i,2)),:);
    end
    line(c1(e(i:i+1,2)),realC0Len-c0(e(i:i+1,1)),'linewidth',3,'color',c);
  end
  
  % c0 labels
%   text(min(c1)-0.8,mean(c0),'C_0','color',c0markerColor);
%   d = [-0.7 0];
%   for(i=1:5:length(c0))
%     text(d(1),c0(i)+d(2),sprintf('%d',i-1));
%     text(max(c1)-d(1),c0(i)+d(2),sprintf('c_{0%d}',mod(i-1,realC0Len)),'color',c0markerColor);
%   end
  
%   % c1 labels
%   text(mean(c1),max(c0)+0.6,'C_1','color',c1markerColor);
%   d = [-0.1 0.3];
%   for(i=1:5:length(c1))
%     text(c1(i)+d(1),c0(1)+d(2),sprintf('%d',i-1));
%     text(c1(i)+d(1),min(c0)-d(2),sprintf('c_{1%d}',mod(i-1,realC1Len)),'color',c1markerColor);
%   end
  
% set up the axes
axis off;
%set(gca,'ylim',[0 .1]);
axis equal;

  
end
