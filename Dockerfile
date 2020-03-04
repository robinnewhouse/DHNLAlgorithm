FROM atlas/analysisbase:21.2.90
COPY --chown=atlas source /code/source
RUN rm -rf /code/source/DHNLAlgorithm/deps/DVAnalysisBase/deps/FactoryTools/dep/ && \
    sudo mkdir /code/build && \
    sudo chown -R atlas /code/build && \
    cd /code/build && \
    source /release_setup.sh && \
    cmake /code/source && \
    make -j4 && \
    source */setup.sh