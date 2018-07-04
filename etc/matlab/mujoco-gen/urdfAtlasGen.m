head = fileread('urdf-template/atlas_head.txt');
body = fileread('urdf-template/atlas_body.txt');
tail = fileread('urdf-template/atlas_tail.txt');

for numrow = 1:5
    % head
    text = head;
    text = strcat(text, '\n');
    
    % body
    i = 0;
    for row = 0:numrow-1
        for col = 0:numrow-1
            base_pos = [row * 2, col * 2, 0.3];
            
            text = strcat(text, ...
                sprintf(body, ...
                i, i, ...
                base_pos(1), base_pos(2), base_pos(3), ...
                i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, ...
                i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, ...
                i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, ...
                i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, ...
                i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, ...
                i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, ...
                i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, ...
                i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i, ...
                i, i, i, i, i, i, i, i, i, i, i, i, i) ...
                );
            
            i = i+1;
        end
    end
    
    % tail
    text = strcat(text, '\n');
    text = strcat(text, tail);
    
    % save file
    fid = fopen(sprintf('robot%d.urdf', numrow * numrow),'wt');
    fprintf(fid, text);
    fclose(fid);
end