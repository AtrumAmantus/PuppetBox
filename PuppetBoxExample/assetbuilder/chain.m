root:
  mesh:
    type: sprite
    offset:
      x: -10
      y: 0
      z: 0
    scale:
      x: 40
      y: 25
    material: Assets1/Materials/Chain-Open
  children:
    linka:
      offset:
        x: -20
        y: 0
        z: 1
      rotation:
        z: 10
      mesh:
        type: sprite
        offset:
          x: -15
          y: 0
        scale:
          x: 40
          y: 10
        material: Assets1/Materials/Chain-Closed
      children:
        linkb:
          offset:
            x: -25
            y: 0
            z: -1
          rotation:
            z: 10
          mesh:
            type: sprite
            offset:
              x: -15
              y: 0
            scale:
              x: 40
              y: 25
            material: Assets1/Materials/Chain-Open
          children:
            linkc:
              offset:
                x: -25
                y: 0
                z: 1
              rotation:
                z: 10
              mesh:
                type: sprite
                offset:
                  x: -15
                  y: 0
                scale:
                  x: 40
                  y: 10
                material: Assets1/Materials/Chain-Closed
              children:
                linkd:
                  offset:
                    x: -25
                    y: 0
                    z: -1
                  rotation:
                    z: 10
                  mesh:
                    type: sprite
                    offset:
                      x: -15
                      y: 0
                    scale:
                      x: 40
                      y: 25
                    material: Assets1/Materials/Chain-Open