if [ ! -f "parameters/$1.sh" ]; then
    echo Invalid data set 1>&2
    exit
fi

. "parameters/$1.sh"

mkdir -p "results/$DATASET_NAME"
for K in 1 10 100; do
    ./exact/tester "data/$DATASET_NAME/train.bin" "data/$DATASET_NAME/test.bin" $DIM $K > "data/$DATASET_NAME/truth_$K"
done

echo -n > "results/$DATASET_NAME/mrpt-voting.txt"
for n_trees in $MRPT_VOTING_N_TREES; do
    for depth in $MRPT_DEPTH; do
        ./mrpt/tester "data/$DATASET_NAME" $DIM $n_trees $depth $MRPT_SPARSITY $MRPT_VOTES >> "results/$DATASET_NAME/mrpt-voting.txt"
    done
done

