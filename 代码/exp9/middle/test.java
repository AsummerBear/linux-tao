package com.exp9;

import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class test {

    private static final String HOST = "10.34.5.215";
    private static final int PORT = 9998;

    public static void main(String[] args) {
        remoteCall();
    }

    private static void remoteCall() {
        String content = null;
        Socket socket = null;
        System.out.println("连接:host=>" + HOST + ",port=>" + PORT);
        try {
            Scanner sc = new Scanner(System.in);
            socket = new Socket(HOST, PORT);
            PrintWriter writer = new PrintWriter(socket.getOutputStream());
            BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream(), "utf-8"));
            // 发送内容
            content = sc.next();
            while (!content.equals("q")){
                writer.print(content+"over");
                String res = "";
                String s = null;
                writer.flush();
                while ((s=reader.readLine())!=null){
                    System.out.println(s);
                    res+=s;
                }
                System.out.println(res);
                content = sc.nextLine();
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (socket != null) {
                    socket.close();
                }
            } catch (IOException e) {
            }
            System.out.println("关闭连接.");
        }
    }
}
