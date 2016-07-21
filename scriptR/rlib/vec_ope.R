###
### vec_ope.R
###

FilterPlus <- function(vec){
    vec.new = vec
    for(ivec in 1:length(vec)){
        if(vec[ivec] < 0.0){
            vec.new[ivec] = 0.0;
        }
    }
    return(vec.new)
}
