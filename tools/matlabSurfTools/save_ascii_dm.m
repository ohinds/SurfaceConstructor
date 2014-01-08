function save_ascii_dm(dm,filename)
  fp = fopen(filename,'w');
  
  sz = size(dm);
  fprintf(fp,'%d %d\n',sz(1),sz(2));
  %fprintf('%d %d\n',sz(1),sz(2));
  
  for(i=1:sz(1))
    for(j=1:sz(2))
      %fprintf('%0.30f ',dm(i,j));
      fprintf(fp,'%0.30f ',dm(i,j));
    end
    fprintf(fp,'\n');
  end
  
  fclose(fp);
  
return;
    