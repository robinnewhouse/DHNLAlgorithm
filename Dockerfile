FROM atlas/analysisbase:21.2.90
COPY --chown=atlas source /code/src
RUN rm -rf /code/src/DHNLAlgorithm/deps/DVAnalysisBase/deps/FactoryTools/dep/ && \
    sudo mkdir /code/build && \
    sudo chown -R atlas /code/build && \
    cd /code/build && \
    source /release_setup.sh && \
    cmake /code/src && \
    make -j && \
    source */setup.sh