def test_version() -> None:
    import ryugraph as ryu

    assert ryu.version != ""
    assert ryu.storage_version > 0
    assert ryu.version == ryu.__version__
