root:
  mesh:
    offset:
      x: -10
      y: 0
      z: 0
    scale:
      x: 40
      y: 25
    material: Assets1/Materials/Chain-Open
    data: Assets1/Mesh/Sprite
  children:
    linka:
      offset:
        x: -20
        y: 0
        z: 1
      rotation:
        z: 10
      mesh:
        offset:
          x: -15
          y: 0
        scale:
          x: 40
          y: 10
        material: Assets1/Materials/Chain-Closed
        data: Assets1/Mesh/Sprite
      children:
        linkb:
          offset:
            x: -25
            y: 0
            z: -1
          rotation:
            z: 10
          mesh:
            offset:
              x: -15
              y: 0
            scale:
              x: 40
              y: 25
            material: Assets1/Materials/Chain-Open
            data: Assets1/Mesh/Sprite
          children:
            linkc:
              offset:
                x: -25
                y: 0
                z: 1
              rotation:
                z: 10
              mesh:
                offset:
                  x: -15
                  y: 0
                scale:
                  x: 40
                  y: 10
                material: Assets1/Materials/Chain-Closed
                data: Assets1/Mesh/Sprite
              children:
                linkd:
                  offset:
                    x: -25
                    y: 0
                    z: -1
                  rotation:
                    z: 10
                  mesh:
                    offset:
                      x: -15
                      y: 0
                    scale:
                      x: 40
                      y: 25
                    material: Assets1/Materials/Chain-Open
                    data: Assets1/Mesh/Sprite