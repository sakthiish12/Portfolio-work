//using System.Collections;
//using System.Collections.Generic;
//using UnityEngine;

//public class PistolControl : MonoBehaviour
//{
//    public Transform FirePoint;
//    public GameObject BulletPrefab;


//    public float BulletForce = 20f;

//    void Start()
//    {
//        FindObjectOfType<PlayerAimWeapon>().Gun1 = this;
//    }


//    public void Shooting()
//    {
//        Debug.Log("IS SHOOTING");
//        GameObject Bullet = Instantiate(BulletPrefab, FirePoint.position, FirePoint.rotation);
//        Rigidbody2D rb = Bullet.GetComponent<Rigidbody2D>();
//        rb.AddForce(FirePoint.right * BulletForce, ForceMode2D.Impulse);
//    }
//}
